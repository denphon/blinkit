// -------------------------------------------------
// BlinKit - BlinKit Library
// -------------------------------------------------
//   File Name: http_response_task.h
// Description: HTTPResponseTask Class
//      Author: Ziming Li
//     Created: 2019-03-22
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

#ifndef BLINKIT_BLINKIT_HTTP_RESPONSE_TASK_H
#define BLINKIT_BLINKIT_HTTP_RESPONSE_TASK_H

#pragma once

#include "sdk/include/BlinKit.h"
#include "response_task.h"

namespace BlinKit {

class CrawlerImpl;

class HTTPResponseTask final : public ResponseTask, public BkRequestClient
{
public:
    HTTPResponseTask(CrawlerImpl &crawler, blink::WebURLLoader *loader, blink::WebURLLoaderClient *client, ResponseData &responseData);

    void Setup(blink::WebTaskRunner *taskRunner) {
        m_taskRunner = taskRunner;
    }
private:
    // BkRequestClient
    void BKAPI RequestComplete(const BkResponse &response) override;
    void BKAPI RequestFailed(int errorCode) override;

    CrawlerImpl &m_crawler;
    blink::WebTaskRunner *m_taskRunner = nullptr;
};

} // namespace BlinKit

#endif // BLINKIT_BLINKIT_HTTP_RESPONSE_TASK_H