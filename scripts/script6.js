let unpipelinedbtn = document.getElementById('unpipelined-button')
let unpipelinedarea = document.getElementById("unpipelined-area")

setTimeout(getfile,100);

function getfile () {
    fetch('../Unpipelined.txt').then(function(response){
        return response.text();
    })
    .then(function(data){
        unpipelinedarea.innerHTML = data;
    })
};
