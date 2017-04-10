#pragma once

#include <d3d12.h>
#include "D3D12Tools.hpp"

// D3D12 timer.
class D3D12Timer {
    public:
        // Constructor.
        D3D12Timer(ID3D12Device* pDevice)
        {
            mpDevice = pDevice;

            mActive = false;
            mDeltaTime = 0;
            mBeginTime = 0;
            mEndTime = 0;
            mQueryCount = 2;

            D3D12_QUERY_HEAP_DESC queryHeapDesc;
            queryHeapDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
            queryHeapDesc.NodeMask = 0;
            queryHeapDesc.Count = mQueryCount;

            ASSERT(mpDevice->CreateQueryHeap(&queryHeapDesc, IID_PPV_ARGS(&mQueryHeap)), S_OK);
        
            {
                D3D12_RESOURCE_DESC resouceDesc;
                ZeroMemory(&resouceDesc, sizeof(resouceDesc));
                resouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
                resouceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
                resouceDesc.Width = sizeof(UINT64) * mQueryCount;
                resouceDesc.Height = 1;
                resouceDesc.DepthOrArraySize = 1;
                resouceDesc.MipLevels = 1;
                resouceDesc.Format = DXGI_FORMAT_UNKNOWN;
                resouceDesc.SampleDesc.Count = 1;
                resouceDesc.SampleDesc.Quality = 0;
                resouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
                resouceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

                ASSERT(mpDevice->CreateCommittedResource(
                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
                    D3D12_HEAP_FLAG_NONE,
                    &resouceDesc,
                    D3D12_RESOURCE_STATE_COPY_DEST,
                    nullptr,
                    IID_PPV_ARGS(&mQueryResource)), S_OK);

                mQueryResource->SetName(L"Query result");
            }
        }

        // Destructor.
        ~D3D12Timer()
        {
            mQueryHeap->Release();
            mQueryResource->Release();
        }

        // Start timestamp.
        void Start(ID3D12GraphicsCommandList* pCommandList)
        {
            assert(!mActive);
            mActive = true;

            pCommandList->EndQuery(mQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, 0);
        }

        // Stop timestamp.
        void Stop(ID3D12GraphicsCommandList* pCommandList)
        {
            assert(mActive);
            mActive = false;

            pCommandList->EndQuery(mQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, 1);

            // Calculate time.
            pCommandList->ResolveQueryData(mQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, 0, 2, mQueryResource, 0);

            // Copy to CPU.
            UINT64 timeStamps[2];
            {
                void* mappedResource;
                CD3DX12_RANGE readRange(0, sizeof(UINT64) * mQueryCount);
                CD3DX12_RANGE writeRange(0, 0);
                ASSERT(mQueryResource->Map(0, &readRange, &mappedResource), S_OK);
                memcpy(&timeStamps, mappedResource, sizeof(UINT64) * mQueryCount);
                mQueryResource->Unmap(0, &writeRange);
            }

            mBeginTime = timeStamps[0];
            mEndTime = timeStamps[1];

            mDeltaTime = mEndTime - mBeginTime;
        }

        // Get time from start to stop in nano seconds.
        unsigned int GetDeltaTime()
        {
            return static_cast<unsigned int>(mDeltaTime);
        }

        unsigned int GetBeginTime()
        {
            return static_cast<unsigned int>(mBeginTime);
        }

        unsigned int GetEndTime()
        {
            return static_cast<unsigned int>(mEndTime);
        }

        // Whether timer is active.
        bool IsActive()
        {
            return mActive;
        }

    private:
        ID3D12Device* mpDevice;
        ID3D12QueryHeap* mQueryHeap;
        ID3D12Resource* mQueryResource;
        bool mActive;
        UINT64 mDeltaTime;
        UINT64 mBeginTime;
        UINT64 mEndTime;
        unsigned int mQueryCount;
};
