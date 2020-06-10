var resetTime = 5000; //Time in milliseconds.

$( document ).ready(function() {
    
    console.log("Starting reset countdown");
    
    setTimeout(function() {
        
        window.location.pathname = "slide/touchscreen/index.html";
        console.log("Redirected to home page.");
        
    }, resetTime);
});