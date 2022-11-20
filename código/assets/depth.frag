#version 300 es

precision mediump float;


in float fieldDoppler;

in vec4 fragColor;

out vec4 outColor;

void main() { 
    
    float x, y, z, w;
    x = fragColor.x;
    y = fragColor.y;
    z = fragColor.z;
    w = 1.0;

    if (fieldDoppler > 55.0){
        
        y -= 0.2;
        z -= 0.2;
    }

    if (fieldDoppler < 17.0){
        
        x -= 0.2;
        y -= 0.2;
    }

    outColor = vec4(x, y, z, w); 

}