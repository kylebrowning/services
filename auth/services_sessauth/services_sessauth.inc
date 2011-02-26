<?php

/**
 * @file
 *  The implementation of the session authentication scheme
 */

function _services_sessauth_authenticate_call($settings, $method, &$args, $auth_args) {
  // Anyone passing in a specific session id to use, should do so in the
  // 'sessid' key. Otherwise, just use the current session (IE do nothing.)
  if (isset($auth_args['sessid'])) {
    // Add additonal processing for methods requiring session
    $session_backup = NULL;
    $sessid = $auth_args['sessid'];
    if (empty($sessid)) {
      return t('Invalid sessid.');
    }
    $session_backup = services_session_load($sessid);
  }
}

function _services_sessauth_security_settings() {
  return array();
}
