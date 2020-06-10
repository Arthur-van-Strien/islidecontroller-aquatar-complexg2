

function sendSelectedTheme(theme) {
    $.getJSON("../img/settings.json", function(data) {
        var Url = data.URL;
        console.log(Url);
        $.get(Url, {thema: theme,optie: 1,foto: false}, function(request) {
            console.log("send");
            location.href = "../themes/succes.html";
        });
        
    });

};

//function only for use when photoshoot is not used
function sendDirectTheme(theme) {
    $.getJSON("img/settings.json", function(data) {
        var Url = data.URL;
        console.log(Url);
        $.get(Url, {thema: theme,optie: 1,foto: false}, function(request) {
            console.log("send");
            location.href = "themes/succes.html";
        });
        
    });

};