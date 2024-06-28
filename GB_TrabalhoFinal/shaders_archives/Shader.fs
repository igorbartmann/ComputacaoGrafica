#version 410

in vec3 finalColor;
in vec3 fragPos;
in vec3 scaledNormal;
in vec2 texCoord;

out vec4 color;

// Textura
uniform sampler2D tex_buffer;

// Propriedades da superfície
uniform vec3 ka; // Ambient reflectivity
uniform vec3 kd; // Diffuse reflectivity
uniform vec3 ks; // Specular reflectivity
uniform vec3 ke; // Emissive color
uniform float d;

// Propriedades da fonte de luz
uniform vec3 lightPos;
uniform vec3 lightColor;

// Posição da Câmera
uniform vec3 cameraPos;

void main()
{
    // Cálculo da parcela de iluminação ambiente
    vec3 ambient = ka * lightColor;

    // Cálculo da parcela de iluminação difusa
    vec3 N = normalize(scaledNormal);
    vec3 L = normalize(lightPos - fragPos);
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = kd * diff * lightColor;

    // Cálculo da parcela de iluminação especular
    vec3 V = normalize(cameraPos - fragPos);
    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(R, V), 0.0), d);
    vec3 specular = ks * spec * lightColor;

    // Sample the texture color
    vec4 texColor = texture(tex_buffer, texCoord);

    // Combine the texture color with the lighting calculations
    vec3 result = (ambient + diffuse) * texColor.rgb + specular;

    // Add emissive color
    result += ke;

    // Set the final color
    color = vec4(result, 1.0);
}
