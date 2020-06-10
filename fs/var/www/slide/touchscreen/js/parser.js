$(document).ready(function(){
    
    $.getJSON("http://127.0.0.1/slide/touchscreen/img/settings.json", function(json) {
        $("#text-yes").html(json.text_yes);
        $("#text-no").html(json.text_no);
        $("#text-permission").html(json.text_permission);
        $("#text-legal").html(json.text_legal);
        $("#text-success").html(json.text_success);
        $("#text-find").html(json.text_find);
    });
});