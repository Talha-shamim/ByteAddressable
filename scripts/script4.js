let pipelinedbtn = document.getElementById('pipelined-button')
let pipelinedarea = document.getElementById("pipelined-area")

setTimeout(getfile,100);

function getfile () {
    fetch('../PipelineWithoutOperandForwarding.txt').then(function(response){
        return response.text();
    })
    .then(function(data){
        pipelinedarea.innerHTML = data;
    })
};
