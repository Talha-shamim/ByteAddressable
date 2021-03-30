let usermembtn = document.getElementById('usermem-button')
let usermemarea = document.getElementById("usermem-area")

setTimeout(getfile,100);

function getfile () {
    fetch('../UserMemory.txt').then(function(response){
        return response.text();
    })
    .then(function(data){
        usermemarea.innerHTML = data;
    })
};
