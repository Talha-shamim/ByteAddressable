let oifbtn = document.getElementById('oif-button')
let oifarea = document.getElementById("oif-area")

setTimeout(getfile,100);

function getfile () {
    fetch('../OtherInfo.txt').then(function(response){
        return response.text();
    })
    .then(function(data){
        oifarea.innerHTML = data;
    })
};
