#define MAKE(Type)                                      \
    [&]() -> decltype(auto) {                           \
        auto instance = Type::make();                   \
        if (!instance) {                                \
            ErrorStates::critical_state();              \
        }                                               \
        return instance.value();                        \
    }()


#define MAKE_WITH_ARGS(Type, ...)                       \
    [&](auto&&... args) -> decltype(auto) {             \
        auto instance = Type::make(                     \
            std::forward<decltype(args)>(args)...       \
        );                                              \
        if (!instance) {                                \
            ErrorStates::critical_state();              \
        }                                               \
        return instance.value();                        \
    }(__VA_ARGS__)


#define TRY_OPTIONAL(func, tag, msg, ...)                           \
    do {                                                            \
        esp_err_t ret = func(__VA_ARGS__);                          \
        if (ret != ESP_OK) {                                        \
            ESP_LOGE(tag, "%s: %s", msg, esp_err_to_name(ret));     \
            return std::nullopt;                                    \
        }                                                           \
    } while(0)


#define TRY_BOOL(func, tag, msg, ...)                               \
    do {                                                            \
        esp_err_t ret = func(__VA_ARGS__);                          \
        if (ret != ESP_OK) {                                        \
            ESP_LOGE(tag, "%s: %s", msg, esp_err_to_name(ret));     \
            return false;                                           \
        }                                                           \
    } while(0)


#define TRY_VOID(func, tag, msg, ...)                               \
    do {                                                            \
        esp_err_t ret = func(__VA_ARGS__);                          \
        if (ret != ESP_OK) {                                        \
            ESP_LOGE(tag, "%s: %s", msg, esp_err_to_name(ret));     \
        }                                                           \
    } while(0)