<?php

/**
 * @file
 * Hooks provided by Services for the definition of servers.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * ...
 *
 */
function hook_rest_server_request_parsers_alter() {
  return array(
    'name' => 'REST',
  );
}

/**
 * ...
 *
 */
function hook_rest_server_response_formatters_alter() {
  return array(
    'name' => 'REST',
  );
}

