#pragma once
#include "CAsset.h"

class CSound :
    public CAsset
{
private:
    FMOD::Sound*            m_Sound;        // Sound 버퍼
    list<FMOD::Channel*>	m_listChannel;  // Sound 가 재생되고 있는 채널 리스트


public:
    // _iRoopCount : 0 (무한반복),  _fVolume : 0 ~ 1(Volume), _bOverlap : 소리 중첩 가능여부
    int Play(int _iRoopCount, float _fVolume, bool _bOverlap, unsigned int _StartPosition = 0);
    void RemoveChannel(FMOD::Channel* _pTargetChannel);
    void Stop();

    // 0 ~ 1
    void SetVolume(float _f, int _iChannelIdx);

    int GetPlaybackPosition();

    FMOD::Channel* GetCurrentChannel()
    {
        if (m_listChannel.empty()) {
            return nullptr; // 채널이 없으면 nullptr 반환
        }

        return m_listChannel.back(); // 가장 최근에 재생된 채널 반환
    }

public:
    virtual int Load(const wstring& _FilePath) override;
    virtual int Save(const wstring& _FilePath) override { return S_OK; }

public:
    CLONE_DISABLE(CSound);
    CSound(bool _EngineRes = false);
    ~CSound();
};

