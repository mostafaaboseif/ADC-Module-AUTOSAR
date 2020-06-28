import json

output = {
    "ID" : {"type" : "ID_number"},
    "ADC" : {"type" : "SELECT", "options" : ["ADC0", "ADC1"]},
    "Sequencer" : {"type" : "SELECT", "options" : ["SS0", "SS1", "SS2", "SS3"]},
    "Trigger Source" : {
        "type" : "SELECT",
        "options" : ["Hardware", "Software"],
        "values" : ["ADC_TRIG_SRC_SW", "ADC_TRIG_SRC_HW"],
        "command" : [{
            "choice" : "Software",
            "type" : "SET",
            "key" : "HW Trigger",
            "value" : "NO_HW_TRIG"
        }]
    },
    "HW Trigger" : {
        "type" : "SELECT",
        "options" : ["PWM Generator", "Analog Comparator", "External GPIO", "Timer"],
        "values" : ["BENDING", "BENDING", "BENDING", "BENDING"],
        "Disable" : [0,3],
    },
    "HW Edge Trigger":{
        "type" : "SELECT",
        "options" : ["Riding Edge", "Falling Edge", "Both Edges"],
        "values" : ["BENDING", "BENDING", "BENDING"]
    },
    "Conversion Mode":{
        "type" : "SELECT",
        "options" : ["Continues", "One Shot"],
        "values" : ["ADC_CONV_MODE_CONTINUOUS", "BENDING"]
    },
    "Acess Mode":{
        "type" : "SELECT",
        "options" : ["Single", "Stream"],
        "values" : ["ADC_CONV_MODE_CONTINUOUS", "BENDING"],
        "command" : [{
            "choice" : "Single",
            "type" : "SET",
            "key" : "Samples per Channel",
            "value" : "1"
        }]
    },
    "Samples per Channel":{
        "type" : "NUMBER"
    },
    "Buffer Type" : {
        "type" : "SELECT",
        "options" : ["Circular", "Linear"],
        "values" : ["BENDING", "BENDING"],
    },
    "IDLE" : {
        "type" : "NONE",
        "value" : "ADC_IDLE"
    },
    "Priority" : {
        "type" : "NUMBER"
    },
}

a = json.dumps(output)
f = open("postBuild.gen", "w")
f.write(a)
f.close()
