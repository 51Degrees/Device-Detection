#[derive(PartialEq, Debug)]
pub enum PlatformName {
    Android,
    MacOS,
    MacOSX,
    iOS,
    Windows,
    Ubuntu
}

impl PlatformName {
    pub fn from_str(value: &str) -> Option<PlatformName>{
        match value{
            "Android" => Some(PlatformName::Android),
            "MacOS" => Some(PlatformName::MacOS),
            "MacOSX" => Some(PlatformName::MacOSX),
            "iOS" => Some(PlatformName::iOS),
            "Windows" => Some(PlatformName::Windows),
            "Ubuntu" => Some(PlatformName::Ubuntu),
            _ => None
        }
    }
}