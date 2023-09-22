/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2017 Intel Corporation
 */

/**
 * @file
 *
 * DPDK Metrics module
 *
 * Metrics are statistics that are not generated by PMDs, and hence
 * are better reported through a mechanism that is independent from
 * the ethdev-based extended statistics. Providers will typically
 * be other libraries and consumers will typically be applications.
 *
 * Metric information is populated using a push model, where producers
 * update the values contained within the metric library by calling
 * an update function on the relevant metrics. Consumers receive
 * metric information by querying the central metric data, which is
 * held in shared memory. Currently only bulk querying of metrics
 * by consumers is supported.
 */

#ifndef _RTE_METRICS_H_
#define _RTE_METRICS_H_

#include <stdint.h>
#include <rte_compat.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Maximum length of metric name (including null-terminator) */
#define RTE_METRICS_MAX_NAME_LEN 64

/**
 * Global metric special id.
 *
 * When used for the port_id parameter when calling
 * rte_metrics_update_metric() or rte_metrics_update_metric(),
 * the global metric, which are not associated with any specific
 * port (i.e. device), are updated.
 */
#define RTE_METRICS_GLOBAL -1


/**
 * A name-key lookup for metrics.
 *
 * An array of this structure is returned by rte_metrics_get_names().
 * The struct rte_metric_value references these names via their array index.
 */
struct rte_metric_name {
	/** String describing metric */
	char name[RTE_METRICS_MAX_NAME_LEN];
};


/**
 * Metric value structure.
 *
 * This structure is used by rte_metrics_get_values() to return metrics,
 * which are statistics that are not generated by PMDs. It maps a name key,
 * which corresponds to an index in the array returned by
 * rte_metrics_get_names().
 */
struct rte_metric_value {
	/** Numeric identifier of metric. */
	uint16_t key;
	/** Value for metric */
	uint64_t value;
};


/**
 * Initializes metric module. This function must be called from
 * a primary process before metrics are used.
 *
 * @param socket_id
 *   Socket to use for shared memory allocation.
 */
void rte_metrics_init(int socket_id);

/**
 * @warning
 * @b EXPERIMENTAL: this API may change without prior notice
 *
 * Deinitialize metric module. This function must be called from
 * a primary process after all the metrics usage is over, to
 *  release the shared memory.
 *
 * @return
 *  -EINVAL - invalid parameter.
 *  -EIO: Error, unable to access metrics shared memory
 *    (rte_metrics_init() not called)
 *  0 - success
 */
__rte_experimental
int rte_metrics_deinit(void);

/**
 * Register a metric, making it available as a reporting parameter.
 *
 * Registering a metric is the way producers declare a parameter
 * that they wish to be reported. Once registered, the associated
 * numeric key can be obtained via rte_metrics_get_names(), which
 * is required for updating said metric's value.
 *
 * @param name
 *   Metric name. If this exceeds RTE_METRICS_MAX_NAME_LEN (including
 *   the NULL terminator), it is truncated.
 *
 * @return
 *  - Zero or positive: Success (index key of new metric)
 *  - -EIO: Error, unable to access metrics shared memory
 *    (rte_metrics_init() not called)
 *  - -EINVAL: Error, invalid parameters
 *  - -ENOMEM: Error, maximum metrics reached
 */
int rte_metrics_reg_name(const char *name);

/**
 * Register a set of metrics.
 *
 * This is a bulk version of rte_metrics_reg_names() and aside from
 * handling multiple keys at once is functionally identical.
 *
 * @param names
 *   List of metric names
 *
 * @param cnt_names
 *   Number of metrics in set
 *
 * @return
 *  - Zero or positive: Success (index key of start of set)
 *  - -EIO: Error, unable to access metrics shared memory
 *    (rte_metrics_init() not called)
 *  - -EINVAL: Error, invalid parameters
 *  - -ENOMEM: Error, maximum metrics reached
 */
int rte_metrics_reg_names(const char * const *names, uint16_t cnt_names);

/**
 * Get metric name-key lookup table.
 *
 * @param names
 *   A struct rte_metric_name array of at least *capacity* in size to
 *   receive key names. If this is NULL, function returns the required
 *   number of elements for this array.
 *
 * @param capacity
 *   Size (number of elements) of struct rte_metric_name array.
 *   Disregarded if names is NULL.
 *
 * @return
 *   - Positive value above capacity: error, *names* is too small.
 *     Return value is required size.
 *   - Positive value equal or less than capacity: Success. Return
 *     value is number of elements filled in.
 *   - Negative value: error.
 */
int rte_metrics_get_names(
	struct rte_metric_name *names,
	uint16_t capacity);

/**
 * Get metric value table.
 *
 * @param port_id
 *   Port id to query
 *
 * @param values
 *   A struct rte_metric_value array of at least *capacity* in size to
 *   receive metric ids and values. If this is NULL, function returns
 *   the required number of elements for this array.
 *
 * @param capacity
 *   Size (number of elements) of struct rte_metric_value array.
 *   Disregarded if names is NULL.
 *
 * @return
 *   - Positive value above capacity: error, *values* is too small.
 *     Return value is required size.
 *   - Positive value equal or less than capacity: Success. Return
 *     value is number of elements filled in.
 *   - Negative value: error.
 */
int rte_metrics_get_values(
	int port_id,
	struct rte_metric_value *values,
	uint16_t capacity);

/**
 * Updates a metric
 *
 * @param port_id
 *   Port to update metrics for
 * @param key
 *   Id of metric to update
 * @param value
 *   New value
 *
 * @return
 *   - -EIO if unable to access shared metrics memory
 *   - Zero on success
 */
int rte_metrics_update_value(
	int port_id,
	uint16_t key,
	const uint64_t value);

/**
 * Updates a metric set. Note that it is an error to try to
 * update across a set boundary.
 *
 * @param port_id
 *   Port to update metrics for
 * @param key
 *   Base id of metrics set to update
 * @param values
 *   Set of new values
 * @param count
 *   Number of new values
 *
 * @return
 *   - -ERANGE if count exceeds metric set size
 *   - -EIO if unable to access shared metrics memory
 *   - Zero on success
 */
int rte_metrics_update_values(
	int port_id,
	uint16_t key,
	const uint64_t *values,
	uint32_t count);

#ifdef __cplusplus
}
#endif

#endif
