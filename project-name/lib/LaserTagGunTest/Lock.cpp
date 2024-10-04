#include "Lock.hpp"
#include "Macros.hpp"

std::optional<Lock> Lock::make(SemaphoreHandle_t mutex){
    if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
        return std::nullopt;
    }

    return std::make_optional<Lock>(mutex);
}

Lock::Lock(SemaphoreHandle_t mutex) : mutex_(mutex){}

Lock::~Lock(){
    if(mutex_ != nullptr){
        if(xSemaphoreGive(mutex_) != pdTRUE){
            ESP_LOGE("Lock", "Failed to give mutex");
        }
    }
}
