let registerbtn = document.getElementById('register-button')
let registerarea = document.getElementById("register-area")

setTimeout(getfile,100);

function getfile () {
    fetch('../Registers.txt').then(function(response){
        return response.text();
    })
    .then(function(data){
        registerarea.innerHTML = data;
    })
};

        
