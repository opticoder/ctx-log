#pragma once

#include <userver/components/loggable_component_base.hpp>
#include <userver/components/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/components/run.hpp>
#include <userver/utils/async.hpp>


void start(bool engine_threads);

namespace ctx_log::test {
	class Component final : public userver::components::LoggableComponentBase {
	public:
		// name of your component to refer in static config
		static constexpr std::string_view kName = "test";

		Component(const userver::components::ComponentConfig& config,
				  const userver::components::ComponentContext& context)
				: userver::components::LoggableComponentBase(config, context) {
			userver::utils::Async("test", [] { start(false); }).Get();
		}

		~Component() = default;
	};
}  // namespace ctx_log::test

template <>
inline constexpr auto userver::components::kConfigFileMode<ctx_log::test::Component> =
		ConfigFileMode::kNotRequired;

void run_userver() {
	userver::components::InMemoryConfig config(R"(
components_manager:
    task_processors:                  # Task processor is an executor for coroutine tasks
        main-task-processor:          # Make a task processor for CPU-bound couroutine tasks.
            worker_threads: 1         # Process tasks in 4 threads.
    default_task_processor: main-task-processor  # Task processor in which components start.
    components:                       # Configuring components that were registered via component_list
        server:
            listener:                 # configuring the main listening socket...
                unix-socket: /tmp/userver_test.sock            # ...to listen on this port and...
                task_processor: main-task-processor    # ...process incoming requests on this task processor.
        logging:
            fs-task-processor: main-task-processor
            loggers:
                default:
                    file_path: '@stdout'
                    level: error
)");
	const auto component_list =
			userver::components::MinimalServerComponentList()
					.Append<ctx_log::test::Component>();
	userver::components::RunOnce(config, component_list);

}
