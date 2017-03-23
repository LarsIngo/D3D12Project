#pragma once

#include <d3d12.h>
#include <assert.h>

// D3D12 timer.
class D3D12Timer {
    public:
        // Constructor.
        D3D12Timer(/*ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext*/)
        {
            //mpDevice = pDevice;
            //mpDeviceContext = pDeviceContext;
            mActive = false;
            mAccurateTime = false;
            mTime = 0.f;

            //D3D11_QUERY_DESC desc;
            //desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
            //desc.MiscFlags = 0;
            //mpDevice->CreateQuery(&desc, &mDisjoint);

            //desc.Query = D3D11_QUERY_TIMESTAMP;
            //mpDevice->CreateQuery(&desc, &mStart);
            //mpDevice->CreateQuery(&desc, &mStop);
        }

        // Destructor.
        ~D3D12Timer()
        {
           /* mDisjoint->Release();
            mStart->Release();
            mStop->Release();*/
        }

        // Start timestamp.
        void Start()
        {
           /* assert(!mActive);
            mActive = true;
            mAccurateTime = false;

            mpDeviceContext->Begin(mDisjoint);
            mpDeviceContext->End(mStart);*/
        }

        // Stop timestamp.
        void Stop()
        {
            /*assert(mActive);
            mActive = false;

            mpDeviceContext->End(mStop);
            mpDeviceContext->End(mDisjoint);*/
        }

        // Stalls CPU/GPU to get results.
        void CalculateTime()
        {
           /* assert(!mActive);

            if (mAccurateTime) return;

            mAccurateTime = true;

            UINT64 startTime = 0;
            while (mpDeviceContext->GetData(mStart, &startTime, sizeof(startTime), 0) != S_OK);

            UINT64 endTime = 0;
            while (mpDeviceContext->GetData(mStop, &endTime, sizeof(endTime), 0) != S_OK);

            D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
            while (mpDeviceContext->GetData(mDisjoint, &disjointData, sizeof(disjointData), 0) != S_OK);

            assert(disjointData.Disjoint == FALSE);

            UINT64 delta = endTime - startTime;
            double frequency = static_cast<double>(disjointData.Frequency);
            mTime = static_cast<float>((delta / frequency));*/
        }

        // Get time from start to stop in seconds.
        float GetTime()
        {
            if (!mAccurateTime) CalculateTime();

            return mTime;
        }

        // Whether timer is active.
        bool IsActive()
        {
            return mActive;
        }

    private:
        //ID3D11Device* mpDevice;
        //ID3D11DeviceContext* mpDeviceContext;
        //ID3D11Query* mDisjoint;
        //ID3D11Query* mStart;
        //ID3D11Query* mStop;
        bool mActive;
        bool mAccurateTime;
        float mTime;
};
