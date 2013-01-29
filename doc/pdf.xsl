<?xml version='1.0'?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:import href="/usr/share/xml/docbook/stylesheet/docbook-xsl/fo/docbook.xsl"/>

<xsl:param name="paper.type" select="'A4'"/>
<xsl:param name="chapter.autolabel" select="1"/>
<xsl:param name="section.autolabel" select="1"/>
<xsl:param name="section.label.includes.component.label" select="2"/>
<xsl:param name="shade.verbatim" select="1"/>
<xsl:param name="title.margin.left" select="'0pc'"/>

<xsl:attribute-set name="shade.verbatim.style">
<xsl:attribute name="padding-top">0.3em</xsl:attribute>
<xsl:attribute name="padding-bottom">0.3em</xsl:attribute>
<xsl:attribute name="padding-start">0.3em</xsl:attribute>
<xsl:attribute name="padding-end">0.3em</xsl:attribute>
<xsl:attribute name="margin-left">0.1em</xsl:attribute>
<xsl:attribute name="margin-right">0.1em</xsl:attribute>
<xsl:attribute name="margin-top">0.1em</xsl:attribute>
<xsl:attribute name="margin-bottom">0.1em</xsl:attribute>
</xsl:attribute-set>

</xsl:stylesheet>
