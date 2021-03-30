let pipelinedbtn = document.getElementById('pipelined-button')
let pipelinedarea = document.getElementById("pipelined-area")

setTimeout(getfile,100);

function getfile () {
    fetch('../output.txt').then(function(response){
        return response.text();
    })
    .then(function(data){
        pipelinedarea.innerHTML = data;
    })
};
