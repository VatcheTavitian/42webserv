function deleteFile(fileName) {
	fetch('/delete-file.html', {
					method: 'DELETE',
					body: fileName,
					headers: {
						'Content-Type': 'text/plain', 
						'File-To-Delete': fileName,
					}
				})
	.then(response => {
		if (response.status === 412)
			alert('File does not exist');
		else if (response.status === 500)
			alert('Server error');
		else if (response.status === 405)
			alert('Server error: Method not allowed');
		else if (response.status === 200)
			alert("File '" + fileName + "'' successfully deleted!")
	})
	.catch(error => {
					console.error('Error uploading file:\n', error.message);     
	});



	console.log("Here")
}



document.addEventListener("DOMContentLoaded", function() {
	const form = document.querySelector("form");
	form.addEventListener("submit", function(event) {
		event.preventDefault();
		const formData = new FormData(form);
		const fileName = formData.get("fileToDelete")
		if (!fileName.length)
			alert("File name can't be empty!");
		else (deleteFile(fileName))
			console.log("Attempting to delete " + fileName);
	});
});
 