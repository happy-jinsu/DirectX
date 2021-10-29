//effect1 
//색 바꾸기
/*struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;  //순서도 잘 지켜야 함.
};

//버텍스 셰이더
VOut VShader(float4 position : POSITION, float4 color : COLOR)
{
    VOut output;

    output.position = position;
    //output.color = color;
    output.color = float4(1.0f,0.0f,1.0f,1.0f);

    return output;
}

//픽셀 셰이더
float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
    return color;
}

*/


//effect2
//투명도? 밝기? 조절
/*struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VOut VShader(float4 position : POSITION, float4 color : COLOR)
{
    VOut output;

    output.position = position;
    output.color = 1.0f - color;    // set the output color to the inverted input color 투명도? 밝기?

    return output;
}


float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
    return color;
}
*/


/*
//effect3
//컬러 하나 정하면 그 톤으로 색이 그라데이션 되어서 나옴.
struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VOut VShader(float4 position : POSITION, float4 color : COLOR)
{
    VOut output;

    output.position = position;
    output.color = color;
    output.color.r = 1.0f;    // maximize the red in each vertex

    return output;
}


float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
    return color;
}
*/




//--------------------------------

//effect4
//작아진다. 스케일 조종
struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VOut VShader(float4 position : POSITION, float4 color : COLOR)
{
    VOut output;

    output.position = position;
    output.position.xy *= 1.0f;    // "shrink" the vertex on the x and y axes
    //1.5로하면 화면에 가득 참.
    output.color = color;
    output.color.r = 1.0f;


    return output;
}


float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
    return color;
}

