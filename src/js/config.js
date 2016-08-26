module.exports = [
  {
    "type": "heading",
    "defaultValue": "App Configuration"
  },
  {
    "type": "text",
    "defaultValue": "Here is some introductory text."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colors"
      },
      {
        "type": "color",
        "messageKey": "BackgroundColor",
        "defaultValue": "0x000000",
        "label": "Background Color"
      },
      {
        "type": "color",
        "messageKey": "ForegroundColor",
        "defaultValue": "0xFFFFFF",
        "label": "Minute Color"
      },
        {
        "type": "color",
        "messageKey": "HourColor",
        "defaultValue": "0xFFFFFF",
        "label": "Hour Color"
      }, 
      {
        "type": "color",
        "messageKey": "DateColor",
        "defaultValue": "0xFFFFFF",
        "label": "Date Color"
      },
      {
        "type": "color",
        "messageKey": "btfColor",
        "defaultValue": "0xFFFFFF",
        "label": "Battery Bar Foreground Color"
      },
       {
        "type": "color",
        "messageKey": "btbColor",
        "defaultValue": "0xFFFFFF",
        "label": "Battery Bar Background Color"
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];