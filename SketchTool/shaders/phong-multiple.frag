#version 140

struct MaterialProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct LightProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec4 position;
    float cosSpotCutoff;
    vec4 spotdirection;
};


in vec3 fNormal;
in vec4 fPosition;
in vec4 fTexCoord;

const int MAXLIGHTS = 10;

uniform MaterialProperties material;
uniform LightProperties light[MAXLIGHTS];
uniform int numLights;

/* texture */
uniform sampler2D image;

out vec4 fColor;

void main()
{
    vec3 lightVec,viewVec,reflectVec;
    vec3 normalView;
    vec3 ambient,diffuse,specular;
    float nDotL,rDotV;


    fColor = vec4(0,0,0,1);

    for (int i=0;i<numLights;i++)
    {
        if (light[i].position.w!=0)
            lightVec = normalize(light[i].position.xyz - fPosition.xyz);
        else
            lightVec = normalize(-light[i].position.xyz);

        vec3 spotdirection = normalize(light[i].spotdirection.xyz);
        if (dot(-lightVec,spotdirection)<light[i].cosSpotCutoff)
            continue;

        vec3 tNormal = fNormal;
        normalView = normalize(tNormal.xyz);


        nDotL = dot(normalView,lightVec);

        viewVec = -fPosition.xyz;
        viewVec = normalize(viewVec);

        reflectVec = reflect(-lightVec,normalView);
        reflectVec = normalize(reflectVec);

        rDotV = max(dot(reflectVec,viewVec),0.0);

        ambient = material.ambient * light[i].ambient;
        diffuse = material.diffuse * light[i].diffuse * max(nDotL,0);
        if (nDotL>0)
            specular = material.specular * light[i].specular * pow(rDotV,material.shininess);
        else
            specular = vec3(0,0,0);
        fColor = fColor + vec4(ambient+diffuse+0*specular,1.0);
        //fColor = fColor + vec4(0*ambient+0*diffuse+0*specular,1.0) + vec4(0.5*(normalView.xyz+1),0);
    }
    fColor = fColor * texture(image,fTexCoord.st);
    fColor = clamp(fColor, 0, 1);
    //fColor = vec4(fTexCoord.s,fTexCoord.t,0,1);
}
