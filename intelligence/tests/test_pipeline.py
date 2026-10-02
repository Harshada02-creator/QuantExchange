import pytest
import pandas as pd
import os
import sys

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..')))
from intelligence.cleaning.validation import validate_ohlcv

def test_missing_fields():
    df = pd.DataFrame({"open": [10], "close": [10]})
    with pytest.raises(ValueError, match="Missing required column"):
        validate_ohlcv(df)

def test_invalid_prices():
    df = pd.DataFrame({
        "timestamp": ["2024-01-01"],
        "symbol": ["AAPL"],
        "open": [-10],
        "high": [10],
        "low": [10],
        "close": [10],
        "volume": [100]
    })
    with pytest.raises(ValueError, match="Invalid non-positive price"):
        validate_ohlcv(df)

def test_invalid_volume():
    df = pd.DataFrame({
        "timestamp": ["2024-01-01"],
        "symbol": ["AAPL"],
        "open": [10],
        "high": [10],
        "low": [10],
        "close": [10],
        "volume": [-100]
    })
    with pytest.raises(ValueError, match="Invalid negative volume"):
        validate_ohlcv(df)

def test_duplicates():
    df = pd.DataFrame({
        "timestamp": ["2024-01-01", "2024-01-01"],
        "symbol": ["AAPL", "AAPL"],
        "open": [10, 10],
        "high": [10, 10],
        "low": [10, 10],
        "close": [10, 10],
        "volume": [100, 100]
    })
    with pytest.raises(ValueError, match="Duplicate timestamp/symbol"):
        validate_ohlcv(df)

def test_valid_data():
    df = pd.DataFrame({
        "timestamp": ["2024-01-01 09:30:00+00:00", "2024-01-01 09:31:00+00:00"],
        "symbol": ["AAPL", "AAPL"],
        "open": [150.0, 150.5],
        "high": [151.0, 152.0],
        "low": [149.5, 150.1],
        "close": [150.5, 151.8],
        "volume": [1000, 1500]
    })
    clean_df = validate_ohlcv(df)
    assert len(clean_df) == 2
