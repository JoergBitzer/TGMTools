#pragma once
#include <vector>

template <class T> class AudioProcessParameter
{
public:
    enum transformerFunc
    {
        notransform,
        db2gaintransform,
        db2powtransform,
        sqrttransform
    };
    AudioProcessParameter(){
        changeTransformer(transformerFunc::notransform);
        
    };
    void prepareParameter(std::atomic<T>* parampointer) {m_param = parampointer;};
    T update(){
        if (*m_param != m_ParamOld)
        {
            m_ParamOld = *m_param;
            m_transformedParamOld =  m_transformParam();
            //return powf(10.f,m_ParamOld/20.f);
        }
        return m_transformedParamOld;
    };
    void changeTransformer(transformerFunc tf)
    {
        switch (tf)
        {
            case transformerFunc::notransform:
                m_transformParam = [this](){return m_ParamOld;};
                break;
            case transformerFunc::db2gaintransform:
                m_transformParam = [this](){return pow(10.0,m_ParamOld/20.0);};
                break;
            case transformerFunc::db2powtransform:
                m_transformParam = [this](){return pow(10.0,m_ParamOld/10.0);};
                break;
            case transformerFunc::sqrttransform:
                m_transformParam = [this](){return sqrt(m_ParamOld);};
                break;

            default:
                m_transformParam = [this](){return m_ParamOld;};
                break;

        }
            
        
    }

private:
    std::atomic<T>* m_param = nullptr; 
    T m_ParamOld = std::numeric_limits<T>::min(); //smallest possible number, will change in the first block
    T m_transformedParamOld = std::numeric_limits<T>::min(); //smallest possible number, will change in the first block

    std::function<T(void)> m_transformParam;

};
