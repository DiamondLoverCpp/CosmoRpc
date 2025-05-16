#include "rpc_zookeeper.h"
#include "rpc_application.h"
#include <semaphore.h>
#include <iostream>

void globle_watcher(zhandle_t *zh, int type, int state, const char *path,void *watcherCtx)
{
    if (type == ZOO_SESSION_EVENT)
    {
        if (state == ZOO_CONNECTED_STATE)
        {
            sem_t *sem = (sem_t *)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}

ZkClient::ZkClient() : m_zhandle(nullptr) {}

ZkClient::~ZkClient()
{
    if (m_zhandle == nullptr)
    {
        zookeeper_close(m_zhandle);
    }
}
void ZkClient::Start()
{
    
}
void ZkClient::Create(const char* path, const char* data, int datalen, int state)
{

}
std::string ZkClient::GetData(const char* path)
{

}

