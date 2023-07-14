#include "fwoop_gaugehistogrammetric.h"
#include "fwoop_gaugemetric.h"
#include <fwoop_openmetrics.h>

#include <fwoop_httpversion.h>
#include <fwoop_log.h>

#include <functional>
#include <memory>

namespace fwoop {

OpenMetricsPublisher::OpenMetricsPublisher(int port) : d_server(port, HttpVersion::Http1_1)
{
    d_server.addRoute("/metrics", std::bind(&OpenMetricsPublisher::handleMetricsEvent, this, std::placeholders::_1,
                                            std::placeholders::_2));
}

OpenMetricsPublisher::~OpenMetricsPublisher() { stop(); }

int OpenMetricsPublisher::start()
{
    d_serverThread = std::thread(&HttpServer::serve, &d_server);
    d_serverThread.detach();
    return 0;
}

void OpenMetricsPublisher::stop()
{
    d_server.stop();
    if (d_serverThread.joinable()) {
        d_serverThread.join();
    }
}

std::shared_ptr<CounterMetricSeries>
OpenMetricsPublisher::newCounterSeries(const std::string &name, const std::string &unit, const std::string &summary)
{
    auto series = std::make_shared<CounterMetricSeries>(name, unit, summary);
    d_series.push_back(series);
    return series;
}

std::shared_ptr<GaugeMetricSeries>
OpenMetricsPublisher::newGaugeSeries(const std::string &name, const std::string &unit, const std::string &summary)
{
    auto series = std::make_shared<GaugeMetricSeries>(name, unit, summary);
    d_series.push_back(series);
    return series;
}

std::shared_ptr<fwoop::GaugeHistogramMetricSeries>
OpenMetricsPublisher::newGaugeHistogramSeries(const std::string &name, const std::string &unit,
                                              const std::string &summary)
{
    auto histogram = std::make_shared<GaugeHistogramMetricSeries>(name, unit, summary);
    d_series.push_back(histogram);
    return histogram;
}

void OpenMetricsPublisher::handleMetricsEvent(const HttpRequest &request, HttpResponse &response)
{
    Log::Debug("got request for metrics");
    response.setStatus("200 OK");
    response.addHeader(fwoop::HttpHeader::ContentType, "text/plain");
    std::string body;
    unsigned int length;
    for (auto metric : d_series) {
        Log::Debug("encoding metric ", metric->name());
        uint8_t *encoding = metric->encode(Metric::OpenMetric, length);
        std::string tmp((char *)encoding, length);
        delete[] encoding;
        body += tmp;
        body += "\n";
    }
    response.setBody(body);
    response.addHeader(fwoop::HttpHeader::ContentLength, std::to_string(body.length()));
}

std::ostream &OpenMetricsPublisher::print(std::ostream &os) const
{
    for (auto metric : d_series) {
        unsigned int length;
        uint8_t *encoding = metric->encode(Metric::OpenMetric, length);
        std::string str((char *)encoding, length);
        delete[] encoding;
        os << str << "\n";
    }
    return os;
}

} // namespace fwoop
