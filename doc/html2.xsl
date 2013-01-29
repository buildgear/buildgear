<?xml version='1.0'?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:import href="/usr/share/xml/docbook/stylesheet/docbook-xsl/html/docbook.xsl"/>

<xsl:param name="generate.toc">book toc,title</xsl:param>
<xsl:param name="chapter.autolabel" select="1"/>
<xsl:param name="section.autolabel" select="1"/>
<xsl:param name="section.label.includes.component.label" select="2"/>

<!-- Remove "Chapter" prefix from chapter titles -->
<xsl:param name="local.l10n.xml" select="document('')"/>
<l:i18n xmlns:l="http://docbook.sourceforge.net/xmlns/l10n/1.0">
  <l:l10n language="en">
    <l:context name="title-numbered">
      <l:template name="chapter" text="%n.&#160;%t"/>
    </l:context>
  </l:l10n>
</l:i18n>

<xsl:template match="section.heading">
  <xsl:variable name="hlevel">
    <xsl:choose>
      <!-- highest valid HTML H level is H6; so anything nested deeper
           than 5 levels down just becomes H6 -->
      <xsl:when test="$level &gt; 5">6</xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$level + 1"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
</xsl:template>

<xsl:template match="refsection/title|refsection/info/title">
  <!-- the ID is output in the block.object call for refsect1 -->
  <xsl:variable name="level" select="count(ancestor-or-self::refsection)"/>
  <xsl:variable name="refsynopsisdiv">
    <xsl:text>0</xsl:text>
    <xsl:if test="ancestor::refsynopsisdiv">1</xsl:if>
  </xsl:variable>
  <xsl:variable name="hlevel">
    <xsl:choose>
      <xsl:when test="$level+$refsynopsisdiv &gt; 5">6</xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$level+1+$refsynopsisdiv"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:element name="h{$hlevel}">
    <xsl:apply-templates/>
  </xsl:element>
</xsl:template>

</xsl:stylesheet> 
