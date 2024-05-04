/*
https://github.com/HandmadeMath/Handmade-Math/blob/master/HandmadeMath.h
https://github.com/recp/cglm/tree/master/include/cglm
https://cglm.readthedocs.io/en/latest/box.html
https://noonat.github.io/intersect/
*/

typedef union M_vec2
{
    struct
    {
        float x, y;
    };
    struct
    {
        float w, h;
    };
    struct
    {
        float width, height;
    };
    struct
    {
        float left, right;
    };
    struct
    {
        float top, bottom;
    };
    struct
    {
        float u, v;
    };
    float vals[2];
#ifdef __cplusplus
    inline float &operator[](const int &i)
    {
        return vals[i];
    }
#endif
} M_vec2;


typedef union M_vec3
{
    struct
    {
        float x, y, z;
    };
    struct
    {
        float u, v, w;
    };
    struct
    {
        float r, g, b;
    };
    struct
    {
        M_vec2 xy;
        float _ignored0_;
    };
    struct
    {
        float _ignored1_;
        M_vec2 YZ;
    };
    struct
    {
        _vec2 UV;
        float _ignored2_;
    };
    struct
    {
        float _ignored3_;
        GFX_vec2 VW;
    };

    float Elements[3];

#ifdef __cplusplus
    inline float &operator[](const int &Index)
    {
        return Elements[Index];
    }
#endif
} hmm_vec3;