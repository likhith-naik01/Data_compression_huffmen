from flask import Flask, request, send_file, jsonify
from flask_cors import CORS
import os
import subprocess
from datetime import datetime

app = Flask(__name__)
CORS(app)  # Enable CORS for frontend

def log_request(message):
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    print(f"[{timestamp}] {message}")

# Test route to verify server is running
@app.route('/')
def home():
    log_request("Health check request received")
    return jsonify({"message": "Huffman Compression Server is running", "status": "ok"})

UPLOAD_FOLDER = os.path.join(os.path.dirname(__file__), 'uploads')
ALGORITHM_PATH = os.path.join(os.path.dirname(__file__), '..', 'algorithm', 'huffman.exe')  # Windows executable

os.makedirs(UPLOAD_FOLDER, exist_ok=True)
print(f"Algorithm path exists: {os.path.exists(ALGORITHM_PATH)}")

def log_debug(message):
    print(f"[DEBUG] {message}")

@app.route('/api/compress', methods=['POST'])
@app.route('/compress', methods=['POST'])
def compress_file():
    try:
        log_request("Compression request received")
        
        # Validate request
        if 'file' not in request.files:
            log_request("Error: No file in request")
            return jsonify({'error': 'No file uploaded'}), 400
        
        if 'file' not in request.files:
            return jsonify({'error': 'No file uploaded'}), 400
        
        file = request.files['file']
        if file.filename == '':
            return jsonify({'error': 'No file selected'}), 400
        
        # Process the file
        filename = file.filename
        if not filename.endswith('.txt'):
            log_request(f"Error: Invalid file type: {filename}")
            return jsonify({'error': 'Please upload a .txt file'}), 400

        # Save with unique name
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        input_path = os.path.join(UPLOAD_FOLDER, f"{timestamp}_{filename}")
        output_filename = f"{timestamp}_{filename.rsplit('.', 1)[0]}.bin"
        output_path = os.path.join(UPLOAD_FOLDER, output_filename)
        
        log_request(f"Saving file: {filename}")
        file.save(input_path)
        
        file_size = os.path.getsize(input_path)
        log_request(f"File saved. Size: {file_size} bytes")
        
        # Call C program
        command = [ALGORITHM_PATH, '-c', input_path, output_path]
        log_debug(f"Running compression command: {' '.join(command)}")
        log_debug(f"Current directory: {os.getcwd()}")
        log_debug(f"Algorithm directory: {os.path.dirname(ALGORITHM_PATH)}")
        
        try:
            result = subprocess.run(
                command,
                capture_output=True,
                text=True,
                cwd=os.path.dirname(ALGORITHM_PATH)  # Run from algorithm directory
            )
        except Exception as e:
            log_debug(f"Exception running compression: {str(e)}")
            raise
        
        log_debug(f"Command exit code: {result.returncode}")
        log_debug(f"Command output: {result.stdout}")
        log_debug(f"Command error: {result.stderr}")
        print(f"Compression command output: {result.stdout}")
        print(f"Compression command error: {result.stderr}")
        
        if result.returncode != 0:
            return jsonify({'error': 'Compression failed: ' + result.stderr}), 500
        
        # Get file sizes
        original_size = os.path.getsize(input_path)
        compressed_size = os.path.getsize(output_path)
        ratio = round(((original_size - compressed_size) / original_size) * 100, 2)
        
        return jsonify({
            'success': True,
            'original_size': original_size,
            'compressed_size': compressed_size,
            'ratio': ratio,
            'download_file': output_filename
        })
    
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/decompress', methods=['POST'])
@app.route('/decompress', methods=['POST'])  # Adding alternative route
def decompress_file():
    try:
        log_request("Decompression request received")
        
        # Validate request
        if 'file' not in request.files:
            log_request("Error: No file in request")
            return jsonify({'error': 'No file uploaded'}), 400
        
        file = request.files['file']
        if file.filename == '':
            log_request("Error: No file selected")
            return jsonify({'error': 'No file selected'}), 400
        
        # Validate file type
        if not file.filename.endswith('.bin'):
            log_request(f"Error: Invalid file type: {file.filename}")
            return jsonify({'error': 'Please upload a compressed .bin file'}), 400
            
        # Save with unique name
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        input_path = os.path.join(UPLOAD_FOLDER, f"{timestamp}_{file.filename}")
        output_filename = f"{timestamp}_{file.filename.rsplit('.', 1)[0]}_decompressed.txt"
        output_path = os.path.join(UPLOAD_FOLDER, output_filename)
        
        log_request(f"Saving file: {file.filename}")
        file.save(input_path)
        
        file_size = os.path.getsize(input_path)
        log_request(f"File saved. Size: {file_size} bytes")
        
        # Call C program
        command = [ALGORITHM_PATH, '-d', input_path, output_path]
        log_debug(f"Running decompression command: {' '.join(command)}")
        
        result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            cwd=os.path.dirname(ALGORITHM_PATH)
        )
        
        log_debug(f"Command exit code: {result.returncode}")
        log_debug(f"Command output: {result.stdout}")
        log_debug(f"Command error: {result.stderr}")
        
        if result.returncode != 0:
            log_request(f"Decompression failed: {result.stderr}")
            os.remove(input_path)  # Clean up input file
            if os.path.exists(output_path):
                os.remove(output_path)  # Clean up any partial output
            return jsonify({'error': 'Decompression failed: ' + result.stderr}), 500
            
        decompressed_size = os.path.getsize(output_path)
        log_request(f"Decompression successful. Output size: {decompressed_size} bytes")
        
        # Clean up input file after successful decompression
        os.remove(input_path)
        
        return jsonify({
            'success': True,
            'originalSize': file_size,
            'decompressedSize': decompressed_size,
            'download_file': output_filename
        })
    
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/download/<filename>')
@app.route('/download/<filename>')  # Adding alternative route
def download_file(filename):
    try:
        log_request(f"Download requested for file: {filename}")
        
        # Validate filename
        if not filename:
            log_request("Error: No filename provided")
            return jsonify({'error': 'No filename provided'}), 400
            
        # Security check - ensure no directory traversal
        if '..' in filename or '/' in filename or '\\' in filename:
            log_request(f"Error: Invalid filename (potential directory traversal): {filename}")
            return jsonify({'error': 'Invalid filename'}), 400
            
        file_path = os.path.join(UPLOAD_FOLDER, filename)
        if not os.path.exists(file_path):
            log_request(f"Error: File not found: {filename}")
            return jsonify({'error': 'File not found'}), 404
            
        # Set correct MIME type based on file extension
        mime_type = 'application/octet-stream'
        if filename.endswith('.txt'):
            mime_type = 'text/plain'
        elif filename.endswith('.bin'):
            mime_type = 'application/octet-stream'
            
        file_size = os.path.getsize(file_path)
        log_request(f"Serving file: {filename} ({file_size} bytes)")
            
        return send_file(
            file_path,
            mimetype=mime_type,
            as_attachment=True,
            download_name=filename
        )
    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    # Check if algorithm exists
    if not os.path.exists(ALGORITHM_PATH):
        print("❌ Error: huffman.exe not found at", os.path.abspath(ALGORITHM_PATH))
        exit(1)
        
    # Ensure upload folder exists
    os.makedirs(UPLOAD_FOLDER, exist_ok=True)
    
    print("\n=== Huffman Compression Server ===")
    print("Algorithm found at:", os.path.abspath(ALGORITHM_PATH))
    print("Upload folder ready at:", os.path.abspath(UPLOAD_FOLDER))
    print("Starting server at: http://localhost:5000")
    print("Press Ctrl+C to stop the server\n")
    
    # Start the server
    app.run(host='localhost', port=5000, debug=True)