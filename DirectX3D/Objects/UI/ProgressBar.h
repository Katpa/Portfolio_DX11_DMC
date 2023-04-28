#pragma once

class ProgressBar : public Quad
{
public:
    ProgressBar(wstring frontImageFile, wstring backImageFile);
    ~ProgressBar();

    void Render();

    void SetAmount(float value);
    void SetAmount(float curHP, float prevHP);

    void SetShader(wstring shaderFile) { material->SetShader(shaderFile); }

private:
    FloatValueBuffer* valueBuffer;
    Texture* backImage;

    float fillAmount = 1.0f;
};