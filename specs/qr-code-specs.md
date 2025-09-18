# QR Code generation into G-Code

Implemented mostly with copilot, but with many details that are needed for it to understand translation between gcode and physical medium

## Copilod Prompt

I'd like to add qr code generation to the web-client. Here are requirements:

- QR codes will require lengths suitable for urls
- QR code needs to be drawn as GCode
- QR-code should open modal, ask for entering the text, and once user submits, should clear current gcode and insert new one
- Label maker draws using variable colros for background and foreground, so we should support inverse mode, where we draw light spaces in qr code, or normal mode, were we draw actual dark spaces
- Single "pixel" should be enough to put as m3;m5 as it will already leave dot on given spot
- For positioning patterns, use actual lines, but lift pen for each line. For outside rectangle of positioning pattern we should only use single line and assume it has required thickness. For inner rect of the positioning pattern, we can fill them using lines (each pixel row is one line)
- inverse mode should include border around the qr code that acts as a background
- bear in mind that actual line and points are sticking by half of the line thickness outward
- line thickness should be used as module size
- if currently displayed thing is qr code, it should recalculate gcode each time the line thickness changes.
- make default url for qr modal to be https://tinyurl.com/5n6y9vjh
- make inverse mode default
- mode selection should be radios, each saying when to use them (light pen on dark paper for inverse and dark pen on light paper for normal)