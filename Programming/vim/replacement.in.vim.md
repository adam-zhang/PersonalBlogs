# VIM部分替换

以下命令将文中所有的字符串idiots替换成managers：
```
:1,$s/idiots/manages/g
```
通常我们会在命令中使用%指代整个文件做为替换范围：
```
:%s/search/replace/g
```
以下命令指定只在第5至第15行间进行替换:
```
:5,15s/dog/cat/g
```
以下命令指定只在当前行至文件结尾间进行替换:
```
:.,$s/dog/cat/g
```
以下命令指定只在后续9行内进行替换:
```
:.,.+8s/dog/cat/g
```
你还可以将特定字符做为替换范围。比如，将SQL语句从FROM至分号部分中的所有等号（=）替换为不等号（<>）：
```
:/FROM/,/;/s/=/<>/g
```
在可视化模式下，首先选择替换范围, 然后输入:进入命令模式，就可以利用s命令在选中的范围内进行文本替换。
