#include <nginx.h>
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
ngx_pool_t *fiftyoneDegrees_ngx_pool;

typedef struct fiftyoneDegreesNginxItems_t {
fiftyoneDegreesDataSet *dataSet;
fiftyoneDegreesResultsetCache *cache;
fiftyoneDegreesWorksetPool *pool;
} fiftyoneDegreesNginxItems;

fiftyoneDegreesNginxItems *fiftyoneDegreesNginxItemsToClean;
