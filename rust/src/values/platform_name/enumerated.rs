#[derive(PartialEq, Debug)]
pub enum PlatformName<'detector> {
    Android,
    MacOS,
    iOS,
    Windows,
    Other(&'detector str),
    Ubuntu,
}

impl<'detector> From<&'detector str> for PlatformName<'detector> {
    fn from(value: &'detector str) -> Self {
        match value {
            "Android" => PlatformName::Android,
            "macOS" => PlatformName::MacOS,
            "iOS" => PlatformName::iOS,
            "Windows" => PlatformName::Windows,
            "Ubuntu" => PlatformName::Ubuntu,
            _ => PlatformName::Other(value)
        }
    }
}
