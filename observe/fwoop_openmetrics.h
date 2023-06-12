#pragma once

#include <fwoop_countermetric.h>
#include <fwoop_gaugehistogrammetric.h>
#include <fwoop_gaugemetric.h>
#include <fwoop_httprequest.h>
#include <fwoop_httpresponse.h>
#include <fwoop_httpserver.h>
#include <fwoop_metric.h>

#include <memory>
#include <ostream>
#include <string>
#include <thread>
#include <vector>

namespace fwoop {

class OpenMetricsPublisher {
  private:
    std::vector<std::shared_ptr<Metric>> d_metrics;
    HttpServer d_server;
    std::thread d_serverThread;

    static uint8_t *encodeCounter(std::shared_ptr<CounterMetric> metric, unsigned int &length);
    void handleMetricsEvent(const HttpRequest &request, HttpResponse &response);

  public:
    OpenMetricsPublisher(int port = 19950);
    ~OpenMetricsPublisher();
    OpenMetricsPublisher(OpenMetricsPublisher &rhs) = delete;
    OpenMetricsPublisher(OpenMetricsPublisher &&rhs) = default;
    OpenMetricsPublisher &operator=(OpenMetricsPublisher &rhs) = delete;

    int start();
    void stop();

    std::shared_ptr<CounterMetric> newCounter(const std::string &name, const std::string &unit,
                                              const std::string &summary = "");
    std::shared_ptr<GaugeMetric> newGauge(const std::string &name, const std::string &unit,
                                          const std::string &summary = "");
    std::shared_ptr<fwoop::GaugeHistogramMetric> newGaugeHistogram(const std::string &name, const std::string &unit,
                                                                   int32_t low, int32_t high, uint16_t numBuckets,
                                                                   const std::string &summary = "");

    std::ostream &print(std::ostream &os) const;
};

} // namespace fwoop
