#pragma once
#include <MeshModel.h>
#include <glm/glm.hpp>
#include <glm\ext\matrix_transform.hpp>

class light : public MeshModel  
{
private:

    glm::vec3 I;
    glm::vec3 N;
    glm::vec3 V;
    glm::vec3 R;
    
    glm::vec3 I_A;
    glm::vec3 I_S;
    glm::vec3 I_D;
    glm::mat4x4 localTransform;
    glm::vec3 position;
    std::string light_type;


public:
    light(glm::vec3 I, glm::vec3 N, glm::vec3 V, glm::vec3 L_A, glm::vec3 L_D, glm::vec3 L_S,
        std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> textureCoords,const std::string& model_name);
    glm::vec3 light::Mul(float x, glm::vec3 vec);
    glm::vec3 L_A;
    glm::vec3 L_S;
    glm::vec3 L_D;
    glm::vec3 paralel;
    void SetTransformX(float val);
    void SetTransformY(float val);
    void SetTransformZ(float val);
    glm::mat4 GetTransform();
    glm::vec3 GetPosVec();
    float GetCosAlpha(glm::vec3 v1, glm::vec3 v2);
    void SetPos(glm::vec3 pos);
    float user_angle;
    void SetActiveLight(std::string light);
    glm::vec3 GetVerAtIndex(int index);
    
    glm::vec3 light::Final_light_gouraud(glm::vec3 K_A, glm::vec3 K_D, glm::vec3 K_S);

    
    void light::Set_I(glm::vec3 I);
    void light::Set_N(glm::vec3 N);
    void light::Set_V(glm::vec3 V);
    void light::Set_R(glm::vec3 R);
    void light::Set_L_A(glm::vec3 L_A);
    void light::Set_L_D(glm::vec3 L_D);
    void light::Set_L_S(glm::vec3 L_S);

    void light::Find_I_A(glm::vec3 K_A); 
    void light::Find_I_S(glm::vec3 K_S, int user_angle);
    void light::Find_I_D(glm::vec3 K_D);
    void Find_I_A_gouraud(glm::vec3 K_A);
    void Find_I_S_gouraud(glm::vec3 K_S, int user_angle);
    void Find_I_D_gouraud(glm::vec3 K_D);

    glm::vec3 light::Final_light(glm::vec3 K_A, glm::vec3 K_D, glm::vec3 K_S, int user_angle, glm::vec3 V);

};