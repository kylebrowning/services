<?php

function services_admin_keys_list() {
  $keys = services_get_keys();
  $header = array(t('Key'), t('Title'), t('Domain'), array('data' => t('Operations'), 'colspan' => '2'));
  $rows = array();

  foreach ($keys as $kid => $key) {
    $row = array();
    
    $row[] = $kid;
    $row[] = $key->title;
    $row[] = $key->domain;
    
    // Populate the operations field.
    $operations = array();

    // Set the edit column.
    $operations[] = array('data' => l(t('edit'), 'admin/build/services/keys/'. $kid));

    // Set the delete column.
    $operations[] = array('data' => l(t('delete'), 'admin/build/services/keys/'. $kid .'/delete'));
    
    //$row = array(array('data' => l($kid, 'admin/build/services/keys/'. $type_url_str), 'class' => $class), array('data' => check_plain($type->type), 'class' => $class), array('data' => check_plain($type->description), 'class' => $class));
    foreach ($operations as $operation) {
      $operation['class'] = $class;
      $row[] = $operation;
    }
    $rows[] = $row;
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No API keys created.'), 'colspan' => '5', 'class' => 'message'));
  }

  return theme('table', $header, $rows);
}

function services_admin_keys_form($key = NULL) {
  $form['kid'] = array(
    '#type' => 'hidden',
    '#default_value' => $key->kid,
  );
  
  if ($key) {
    $form['key'] = array(
      '#type' => 'markup',
      '#title' => t('Key'),
      '#value' => '<strong>' . t('API Key') . ':</strong> ' . $key->kid,
    ); 
  }
  
  $form['title'] = array(
    '#title' => t('Application title'),
    '#type' => 'textfield',
    '#default_value' => $key->title,
    '#description' => t('The title of the application or website using the service.'),
  );
  $form['domain'] = array(
    '#title' => t('Allowed domain'),
    '#type' => 'textfield',
    '#default_value' => $key->domain,
    '#description' => t('External domain allowed to use this key. (One domain per line)'),
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => ($key) ? t('Save key') : t('Create key'),
  );
  
  return $form;
}

function services_admin_keys_form_validate() {
  
}

function services_admin_keys_form_submit($form_id, $form_values) {
  services_admin_keys_save($form_values);
  
  return 'admin/build/services/keys';
}

function services_admin_keys_save($key) {
  $is_existing = FALSE;
  $key['kid'] = !empty($key['kid']) ? $key['kid'] : md5(uniqid(mt_rand(), true));
  $is_existing = db_num_rows(db_query("SELECT * FROM {api_keys} WHERE kid = '%s'", $key['kid']));
  
  if (!$is_existing) {
    db_query("INSERT INTO {api_keys} (kid, title, domain) VALUES ('%s', '%s', '%s')", $key['kid'], $key['title'], $key['domain']);
    return SAVED_NEW;
  }
  else {
    db_query("UPDATE {api_keys} SET title = '%s', domain = '%s' WHERE kid = '%s'", $key['title'], $key['domain'], $key['kid']);
    return SAVED_UPDATED;
  }
}

function services_admin_keys_delete($kid) {
  db_query("DELETE FROM {api_keys} WHERE kid = '%s'", $kid);
}

function services_admin_keys_delete_confirm($key) {
  $form['kid'] = array('#type' => 'value', '#value' => $key->kid);

  $message = t('Are you sure you want to delete the key %key?', array('%key' => $key->kid));
  $caption = '';

  $caption .= '<p>'. t('This action cannot be undone.') .'</p>';

  return confirm_form($form, $message, 'admin/build/services/keys', $caption, t('Delete'));
}

function services_admin_keys_delete_confirm_submit($form_id, $form_values) {
  services_admin_keys_delete($form_values['kid']);

  $t_args = array('%key' => $form_values['kid']);
  drupal_set_message(t('The key %key has been deleted.', $t_args));
  watchdog('menu', t('Deleted key %key.', $t_args), WATCHDOG_NOTICE);

  return 'admin/build/services/keys';
}