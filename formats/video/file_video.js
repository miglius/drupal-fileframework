// $Id$

Drupal.file_videoFrameLink = function(frame) {
  var url = $('#file-video-select-frame-link').attr('href');
  $('#file-video-select-frame-link').attr('href', url.replace(/frame\/.*/, 'frame/' + frame));
}

