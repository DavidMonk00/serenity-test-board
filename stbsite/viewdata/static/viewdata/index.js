var statusIntervalId = window.setInterval(checkFTDIStatus, 1000);

function checkFTDIStatus() {
    $.ajax({
        method: "GET",
        url: "/viewdata/check_ftdi_status",
        data: {},
        success: function(data) {
            console.log(data)
            if (data.ftdi_connected == true) {
                $("#ftdi-status").css({ color: "green" }).text("online");
            } else {
                $("#ftdi-status").css({ color: "red" }).text("offline");
            }
        }
    });
};

function checkI2CStatus() {
    $.ajax({
        method: "GET",
        url: "/viewdata/check_i2c_status",
        data: {},
        success: function(data) {
            console.log(data)
            if (data.i2c_fanout_connected == true) {
                $("#i2c-fanout-status").css({ color: "green" }).text("online");
            } else {
                $("#i2c-fanout-status").css({ color: "red" }).text("offline");
            }
            if (data.adc_connected == 'ERROR') {
                $("#adc-status").css({ color: "red" }).text(data.adc_connected);
            } else {
                $("#adc-status").css({ color: "black" }).text(data.adc_connected);
            }
        }
    });
};
