// $Id$

Drupal.behaviors.file_gallery = function(context) {
  $('select#file-gallery-selector').change(function() {
    var filter = this.options[this.selectedIndex].value;
    var url = window.location.href.replace(/([file_gallery|gallery]\/[\d]+\/[\d]+)(\/{0,1}[\w]*\/{0,1})/, filter ? '$1/' + filter : '$1');
    window.location.href = url;
  });
};

