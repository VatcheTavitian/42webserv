        document.addEventListener("DOMContentLoaded", function() {
            const form = document.querySelector("form");
            form.addEventListener("submit", function(event) {
                event.preventDefault(); // Prevent the default form submission
    
                const input = document.getElementById("file");
                const file = input.files[0];
    
                if (file) {
                    console.log('Uploading file:', file.name);
                    let reader = new FileReader();
                    reader.onload = function(e) {
                        const base64File = btoa(e.target.result); // Convert file content to base64
    
                        fetch('/upload.html', {
                            method: 'POST',
                            body: base64File,
                            headers: {
                                'Content-Type': file.type, // Set content type based on file type
                                'Content-Disposition': `attachment; filename="${file.name}"`,
                            }
                        })
                        .then(response => {
                            console.log(response.status);
                            if (response.status === 413 ) {
                                alert('File upload unsuccessful!!\nFile size not permitted!');
                                throw new Error('File size not permitted!');
                            } 
                            else if (response.status === 500 ) {
                                alert('File upload unsuccessful!!\nInternal Server Error!');
                                throw new Error('File size not permitted!');
                            } 
                            else if (!response.ok) {
                                alert('Error Uploading File!');
                                throw new Error('Error uploading file');
                            } else if (response.status === 200) {
                                alert('File uploaded successfully');
                                window.location.href = '/upload ';
                            }
                            return response.text();
                        })
                        .then(data => {
                            console.log('File uploaded successfully. Response from server:', data);
                        })
                        .catch(error => {
                            console.error('Error uploading file:\n', error.message);
                        });
                    };
                    reader.onerror = function(e) {
                        console.error('Error reading file:', e.target.error);
                    };
                    reader.readAsBinaryString(file); // Read file as binary string
                } else {
                    alert('Please select a file to upload.');
                }
            });
        });