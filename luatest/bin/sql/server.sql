CREATE TABLE `account` (
  `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `username` varchar(32) NOT NULL,
  `password` varchar(32) NOT NULL,
  `register_time` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312
