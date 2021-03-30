let filebtn = document.getElementById('file-button')
let filearea = document.getElementById("file-area")

setTimeout(getfile,100);

function getfile () {
    fetch('../input.txt').then(function(response){
        return response.text();
    })
    .then(function(data){
        filearea.innerHTML = data;
    })
};
