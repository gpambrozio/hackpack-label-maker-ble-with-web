#!/usr/bin/env python3
"""
Simple HTTP server to serve the label maker web client locally.
This resolves CORS issues when loading local font files.

Usage:
    python3 serve.py

Then open http://localhost:8000/web-client.html in your browser.
"""

import http.server
import socketserver
import webbrowser
import os
import sys

PORT = 8000

class CustomHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        # Add CORS headers for font files
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', '*')
        super().end_headers()

def main():
    # Change to the directory containing this script
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    with socketserver.TCPServer(("", PORT), CustomHTTPRequestHandler) as httpd:
        print(f"Serving at http://localhost:{PORT}")
        print(f"Open http://localhost:{PORT}/web-client.html in your browser")
        print("Press Ctrl+C to stop the server")

        # Try to open the browser automatically
        try:
            webbrowser.open(f'http://localhost:{PORT}/web-client.html')
        except:
            pass

        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\nServer stopped.")
            sys.exit(0)

if __name__ == "__main__":
    main()