let pdfbtn = document.getElementById('pdf-button')
let pdfarea = document.getElementById("pdf-area")

setTimeout(getfile,100);

function getfile () {
    fetch('../PipelineWithOperandForwarding.txt').then(function(response){
        return response.text();
    })
    .then(function(data){
        pdfarea.innerHTML = data;
    })
};
