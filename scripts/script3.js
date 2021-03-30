let membtn = document.getElementById('mem-button')
let memarea = document.getElementById("mem-area")

setTimeout(getfile,100);

function getfile () {
    fetch('../InstructionMemory.txt').then(function(response){
        return response.text();
    })
    .then(function(data){
        memarea.innerHTML = data;
    })
};
