#include <iostream>
#include <string>
#include <benchmark/benchmark.h>

const int QUEUE_SIZE = 8192;

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/initializer.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/asyncappender.h>
//********************************************
// log4cplus
//********************************************
log4cplus::Initializer initializer;
log4cplus::Logger GetLog4cplusLogger(bool async, bool file) {
    log4cplus::tstring name;
    log4cplus::SharedAppenderPtr appender;
    if (file) {
        appender = new log4cplus::FileAppender(LOG4CPLUS_TEXT("log4cplus.txt"));
        name = LOG4CPLUS_TEXT("file_logger");
    }
    else {
        appender = new log4cplus::ConsoleAppender();
        name = LOG4CPLUS_TEXT("console_logger");
    }
    appender->setName(name);
    appender->setLayout(std::unique_ptr<log4cplus::PatternLayout>(new log4cplus::PatternLayout(
        LOG4CPLUS_TEXT("[%D{%Y-%m-%d %H:%M:%S.%Q}] [%c{1}] [%p] %m%n"))));
    log4cplus::Logger logger = log4cplus::Logger::getInstance(name);
    logger.removeAllAppenders();
    if (async) {
        log4cplus::SharedAppenderPtr async_appender(new log4cplus::AsyncAppender(appender, QUEUE_SIZE));
        logger.addAppender(async_appender);
    }
    else {
        logger.addAppender(appender);
    }
    return logger;
}


auto BM_log4cplus = [](benchmark::State& state, bool async, bool file) {
    log4cplus::Logger logger = GetLog4cplusLogger(async, file);
    log4cplus::tstring msg(state.range(0), 'l');
    for (auto _ : state) {
        auto start = std::chrono::high_resolution_clock::now();
        LOG4CPLUS_INFO(logger, msg);
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed_seconds =
            std::chrono::duration_cast<std::chrono::duration<double>>(
                end - start);
        state.SetIterationTime(elapsed_seconds.count());
    }
    auto start = std::chrono::high_resolution_clock::now();
    logger.shutdown();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds =
        std::chrono::duration_cast<std::chrono::duration<double>>(
            end - start);
    state.SetIterationTime(elapsed_seconds.count());
    state.counters["flush"] = elapsed_seconds.count();
    state.SetItemsProcessed(state.iterations());
};



#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/async.h"
//********************************************
//spdlog
//********************************************

std::shared_ptr<spdlog::logger> GetSpdlogLogger(bool asyn, bool file) {
    if (asyn) {
        spdlog::init_thread_pool(QUEUE_SIZE, 1);
        if (file) {
            return spdlog::basic_logger_mt<spdlog::async_factory>("file_logger", "spdlog.txt", true);
        }
        else {
            return spdlog::stdout_logger_mt<spdlog::async_factory>("console_logger");
        }
    }
    else {
        if (file) {
            return spdlog::basic_logger_mt("file_logger", "spdlog.txt", true);
        }
        else {
            return spdlog::stdout_logger_mt("console_logger");
        }
    }
}

auto BM_spdlog = [](benchmark::State& state, bool asyn, bool file) {
    auto logger = GetSpdlogLogger(asyn, file);
    std::string msg(state.range(0), 's');
    for (auto _ : state) {
        auto start = std::chrono::high_resolution_clock::now();
        logger->info(msg);
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed_seconds =
            std::chrono::duration_cast<std::chrono::duration<double>>(
                end - start);
        state.SetIterationTime(elapsed_seconds.count());
    }
    auto start = std::chrono::high_resolution_clock::now();
    logger->flush();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds =
        std::chrono::duration_cast<std::chrono::duration<double>>(
            end - start);
    state.SetIterationTime(elapsed_seconds.count());
    state.counters["flush"] = elapsed_seconds.count();
    state.SetItemsProcessed(state.iterations());
    spdlog::drop(logger->name());
};




//********************************************
//boost
//********************************************
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/core.hpp> 
#include <boost/log/expressions.hpp> 
#include <boost/log/sinks/async_frontend.hpp> 
#include <boost/log/sinks/sync_frontend.hpp> 
#include <boost/log/sinks/text_ostream_backend.hpp> 
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/severity_channel_logger.hpp> 
#include <boost/log/sources/record_ostream.hpp> 
#include <boost/log/trivial.hpp> 
#include <boost/log/support/date_time.hpp> 
#include <boost/log/utility/setup/common_attributes.hpp> 
#include <boost/log/sources/severity_logger.hpp> 
#include <boost/log/attributes/current_thread_id.hpp> 
#include <boost/log/attributes/current_process_name.hpp> 
#include <boost/log/attributes/attribute.hpp> 
#include <boost/log/attributes/attribute_cast.hpp> 
#include <boost/log/attributes/attribute_value.hpp> 
#include <boost/log/sinks/async_frontend.hpp> 
// Related headers 
#include <boost/log/sinks/unbounded_fifo_queue.hpp> 
#include <boost/log/sinks/unbounded_ordering_queue.hpp> 
#include <boost/log/sinks/bounded_fifo_queue.hpp> 
#include <boost/log/sinks/bounded_ordering_queue.hpp> 
#include <boost/log/sinks/drop_on_overflow.hpp> 
#include <boost/log/sinks/block_on_overflow.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;

enum severity_level { normal, info, warning, error, critical };
std::ostream &operator<<(std::ostream &strm, severity_level level) {
    static const char *strings[] = { "normal", "info", "warning", "error", "critical" };
    if (static_cast<std::size_t>(level) < sizeof(strings) / sizeof(*strings))
        strm << strings[level];
    else
        strm << static_cast<int>(level); return strm;
}


class BoostLoggerInit
{
    typedef sinks::asynchronous_sink<sinks::text_ostream_backend, sinks::bounded_fifo_queue<QUEUE_SIZE, sinks::block_on_overflow>> asyn_sink_t;
    typedef sinks::synchronous_sink<sinks::text_ostream_backend> sync_sink_t;

public:
    BoostLoggerInit(bool async, bool file)
    {
        if (async) {
            _asyn_sink = init_logging<asyn_sink_t>(file);
        }
        else {
            _sync_sink = init_logging<sync_sink_t>(file);
        }
    }

    ~BoostLoggerInit()
    {
        stop_logging(_asyn_sink);
        stop_logging(_sync_sink);
    }

    void Flush()
    {
        if (_asyn_sink) _asyn_sink->flush();
        if (_sync_sink) _sync_sink->flush();
    }
private:
    template<typename SinkType>
    boost::shared_ptr<SinkType> init_logging(bool file) {
        logging::add_common_attributes();
        boost::shared_ptr<logging::core> core = logging::core::get();
        boost::shared_ptr<sinks::text_ostream_backend> backend = boost::make_shared<sinks::text_ostream_backend>();
        if (file)
            backend->add_stream(boost::make_shared<std::ofstream>("boostlog.txt"));
        else
            backend->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter()));
        boost::shared_ptr<SinkType> sink(new SinkType(backend));
        core->add_sink(sink);
        sink->set_filter(expr::attr<severity_level>("Severity") >= info);
        std::string name = "console_logger";
        if (file) name = "file_logger";
        sink->set_formatter(
            expr::stream
            << "[" << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
            << "] [" << name
            << "] [" << expr::attr<severity_level>("Severity")
            << "] "
            << expr::smessage);
        return sink;
    }

    void stop_logging(boost::shared_ptr<asyn_sink_t> &sink) {
        if (!sink) {
            return;
        }
        boost::shared_ptr<logging::core> core = logging::core::get();
        // Remove the sink from the core, so that no records are passed to it 
        core->remove_sink(sink);
        // Break the feeding loop 
        sink->stop();
        // Flush all log records that may have left buffered 
        sink->flush();
        sink.reset();
    }

    void stop_logging(boost::shared_ptr<sync_sink_t> &sink) {
        if (!sink) {
            return;
        }
        boost::shared_ptr<logging::core> core = logging::core::get();
        // Remove the sink from the core, so that no records are passed to it 
        core->remove_sink(sink);
        // Break the feeding loop 
        //sink->stop();
        // Flush all log records that may have left buffered 
        sink->flush();
        sink.reset();
    }

    boost::shared_ptr<asyn_sink_t> _asyn_sink;
    boost::shared_ptr<sync_sink_t> _sync_sink;
};


auto BM_boostlog = [](benchmark::State& state, bool asyn, bool file) {
    BoostLoggerInit sink(asyn, file);
    src::severity_logger_mt<severity_level> lg;
    std::string msg(state.range(0), 'b');
    for (auto _ : state) {
        auto start = std::chrono::high_resolution_clock::now();
        BOOST_LOG_SEV(lg, info) << msg;
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed_seconds =
            std::chrono::duration_cast<std::chrono::duration<double>>(
                end - start);
        state.SetIterationTime(elapsed_seconds.count());
    }
    auto start = std::chrono::high_resolution_clock::now();
    sink.Flush();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds =
        std::chrono::duration_cast<std::chrono::duration<double>>(
            end - start);
    state.SetIterationTime(elapsed_seconds.count());
    state.counters["flush"] = elapsed_seconds.count();
    state.SetItemsProcessed(state.iterations());
};



// main
int main(int argc, char** argv) {
    // log4cplus
    auto inputs_log4cplus = { std::make_tuple("l4cp_a_c", true, false),
        std::make_tuple("l4cp_a_f", true, true),
        std::make_tuple("l4cp_s_c", false, false),
            std::make_tuple("l4cp_s_f", false, true) };
    for (auto& input : inputs_log4cplus)
        benchmark::RegisterBenchmark(std::get<0>(input), BM_log4cplus, std::get<1>(input), std::get<2>(input))->UseManualTime()->Arg(32)->Arg(512);
    // spdlog
    auto inputs_spdlog = { std::make_tuple("spd_a_c", true, false),
        std::make_tuple("spd_a_f", true, true),
        std::make_tuple("spd_s_c", false, false),
            std::make_tuple("spd_s_f", false, true) };
    for (auto& input : inputs_spdlog)
        benchmark::RegisterBenchmark(std::get<0>(input), BM_spdlog, std::get<1>(input), std::get<2>(input))->UseManualTime()->Arg(32)->Arg(512);
    // boostlog
    auto inputs_boostlog = { std::make_tuple("boost_a_c", true, false),
        std::make_tuple("boost_a_f", true, true),
        std::make_tuple("boost_s_c", false, false),
            std::make_tuple("boost_s_f", false, true) };
    for (auto& input : inputs_boostlog)
        benchmark::RegisterBenchmark(std::get<0>(input), BM_boostlog, std::get<1>(input), std::get<2>(input))->UseManualTime()->Arg(32)->Arg(512);
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
}

